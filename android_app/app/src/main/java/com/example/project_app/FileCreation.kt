Button createFileButton = findViewById(R.id.create_file_button);

createFileButton.setOnClickListener(new View.OnClickListener() {
    @Override
    public void onClick(View v) {
        // Créez un fichier dans le répertoire de fichiers externes de l'application
        File file = new File(getExternalFilesDir(null), "example.txt");

        //Cliquez sur "View" dans la barre de menus, puis sélectionnez "Tool Windows" et "Device File Explorer".
        //Dans la fenêtre "Device File Explorer", on peut naviguer dans les fichiers de l'émulateur en utilisant l'arborescence de fichiers.

        try {
            // Écrivez du texte dans le fichier
            FileWriter writer = new FileWriter(file);
            writer.append("Contenu du fichier");
            writer.flush();
            writer.close();

            Toast.makeText(getApplicationContext(), "Fichier créé avec succès", Toast.LENGTH_LONG).show();
        } catch (IOException e) {
            e.printStackTrace();
            Toast.makeText(getApplicationContext(), "Erreur lors de la création du fichier", Toast.LENGTH_LONG).show();
        }
    }
});
