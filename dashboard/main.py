import streamlit as st
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import folium
from streamlit_extras.let_it_rain import rain


# Charger le fichier CSV
df = pd.read_csv('results.csv', sep=',', header=0)
df = df.sort_values(by='first_detection')
# Titre de l'application
st.title("Application de Visualisation")

# Onglets
st.sidebar.title("Options")
onglets = st.sidebar.radio("Choisir une vue", ["Carte interactive", "Statistiques globales", "Analyse temporelle","Priorité de réparation","Performance de l'équipe de réparation","Réparation"])

# Onglet "Carte interactive"
if onglets == "Réparation":
    st.title("Réparation des dégradations")
    df['index'] = df.index.astype(str)

    # Sélectionner une dégradation
    selected_degradation = st.selectbox("Sélectionner une dégradation", df['index'])

    # Afficher l'état actuel de la dégradation
    current_state = df[df['index'] == selected_degradation]['state'].values[0]
    st.write(f"État actuel de la dégradation '{selected_degradation}': {current_state}")

    # Bouton pour effectuer la réparation
    if st.button("Réparer la dégradation"):
        # Mettre à jour l'état de la dégradation dans le DataFrame
        df.loc[df['index'] == selected_degradation, 'state'] = 'repare'
        df.to_csv('results.csv', index=False)

        st.success(f"La dégradation '{selected_degradation}' a été réparée avec succès!")
        rain(
        emoji="🎉",
        font_size=54,
        falling_speed=5,
        animation_length=0.3,
    )

if onglets == "Carte interactive":
    # Ajouter une liste déroulante pour choisir le type de dégradation
    type_degradation_selection = st.selectbox("Choisir le type de dégradation", ['all'] + list(df['type'].unique()))

    # Ajouter un curseur pour définir à partir de combien de "number_occurrence" on affiche le point
    nombre_occurrence_min = st.slider("Nombre minimum d'occurrences", min_value=0, max_value=df['number_of_detection'].max(), value=0)

    # Filtrer le DataFrame en fonction du type de dégradation et du nombre d'occurrences sélectionnés
    if type_degradation_selection == 'all':
        df_filtre = df[df['number_of_detection'] >= nombre_occurrence_min]
    else:
        df_filtre = df[(df['type'] == type_degradation_selection) & (df['number_of_detection'] >= nombre_occurrence_min)]

    # Créer une carte interactive avec Plotly Express
    fig = px.scatter_mapbox(
        df_filtre,
        lat='latitude',
        lon='longitude',
        color='state',
        text='type',
        color_discrete_map={'presente': 'red', 'repare': 'green'},
        size_max=10,
        zoom=10
    )

    # Configurer le style de la carte
    fig.update_layout(mapbox_style='open-street-map')

    # Afficher la carte dans Streamlit
    st.plotly_chart(fig)

# Onglet "Statistiques globales"
elif onglets == "Statistiques globales":
    # Ajouter des statistiques globales
    st.subheader("Statistiques globales")
    st.write("Nombre total de points :", df.shape[0])
    st.write("Nombre total de points 'presente' :", df[df['state'] == 'presente'].shape[0])
    st.write("Nombre total de points 'repare' :", df[df['state'] == 'repare'].shape[0])

# Onglet "Analyse temporelle"
elif onglets == "Analyse temporelle":
        # Créer une colonne 'variation' basée sur la colonne 'etat'
    df['variation'] = df['state'].apply(lambda x: 1 if x == 'presente' else -1)

    # Créer une colonne cumulée pour suivre l'évolution au fil du temps
    df['cumulative_variation'] = df['variation'].cumsum()

    # Créer une figure avec Plotly
    fig = go.Figure()

    # Ajouter la ligne d'évolution
    fig.add_trace(go.Scatter(x=df['first_detection'], y=df['cumulative_variation'], mode='lines', name='Évolution'))

    # Configurer le titre et les axes
    fig.update_layout(title='Évolution du nombre de dégradations au fil du temps',
                    xaxis_title='Date',
                    yaxis_title='Évolution cumulative')

    # Afficher la figure dans Streamlit
    st.plotly_chart(fig)

        # Ajouter une colonne 'mois' en extrayant le mois à partir de la date
    df['mois'] = pd.to_datetime(df['first_detection']).dt.month

    # Créer un graphique en barres groupées en fonction du mois, du type de dégradation et du nombre d'occurrences
    fig_tendances_saisonnieres = px.bar(df, x='mois', color='type', y=df.groupby(['mois', 'type']).cumcount() + 1, barmode='group',
                                        labels={'mois': 'Mois', 'number_of_detection': 'Nombre de dégradations'},
                                        title='Tendances saisonnières en fonction du nombre de dégradations et de leur type')

    # Configurer le style du graphique
    fig_tendances_saisonnieres.update_layout(xaxis_title='Mois', yaxis_title='Nombre de dégradations')

    # Afficher le graphique dans Streamlit
    st.plotly_chart(fig_tendances_saisonnieres)

if onglets == "Priorité de réparation":
        # Créer une liste unique des types de dégradation
    types_degradation_uniques = df['type'].unique()

    # Paramètres
    couts_unitaires = {}
    for type_degradation in types_degradation_uniques:
        cout_unitaire = st.slider(f"Coût unitaire pour {type_degradation}", min_value=1, max_value=100, value=50)
        couts_unitaires[type_degradation] = cout_unitaire

    importance_cout_usagers = st.slider("Importance du coût par rapport au nombre d'usagers impactés", min_value=1, max_value=100, value=50)
    # Filtrer les dégradations non réparées
    df_non_repare = df[df['state'] != 'repare']

    # Calculer le coût total pour chaque dégradation non réparée
    df_non_repare['cout_total'] = df_non_repare['type'].map(couts_unitaires)

    # Calculer la métrique de priorité pour chaque dégradation non réparée
    df_non_repare['priorite'] = (df_non_repare['number_of_detection']*importance_cout_usagers/100) + df_non_repare['cout_total']

    # Trier les dégradations non réparées en fonction de la métrique de priorité
    df_priorite = df_non_repare.sort_values(by='priorite', ascending=False)

    # Afficher le DataFrame trié
    st.write(df_priorite)

    # Ajouter une colonne pour les liens de carte
    df_priorite['lien_carte'] = df_priorite.index.astype(str)

    # Sélectionner l'index lorsqu'il est cliqué
    selected_index = st.selectbox('Sélectionnez un index pour afficher la carte', df_priorite.index, format_func=lambda x: str(x))

    if st.button('Afficher la carte'):
        # Récupérer la ligne sélectionnée
        selected_degradation = df_priorite.loc[int(selected_index)]

        # Créer un DataFrame spécifique pour la ligne sélectionnée
        df_selected = pd.DataFrame(selected_degradation).T


        # Créer la carte avec le DataFrame spécifique
        fig = px.scatter_mapbox(
            df_selected,
            lat='latitude',
            lon='longitude',
            color='state',
            text='type',  # Assurez-vous que 'type' est une colonne dans votre DataFrame
            color_discrete_map={'presente': 'red', 'repare': 'green'},
            size_max=10,
            zoom=10
        )

        # Configurer le style de la carte
        fig.update_layout(mapbox_style='open-street-map')

        # Afficher la carte dans Streamlit
        st.plotly_chart(fig)