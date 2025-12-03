import pandas as pd
import matplotlib.pyplot as plt
import os


def generate_reliabilityWEPH_image(csv_file_path, rounds=[1, 4, 10], output_dir="images"):


    # Charger les données
    df = pd.read_csv(csv_file_path)
    df.columns = df.columns.str.strip()  # Supprimer espaces invisibles

    # Vérifier les colonnes
    required_cols = ["WEP", "r", "k", "Result"]
    for col in required_cols:
        if col not in df.columns:
            raise ValueError(f"Colonne manquante dans CSV : {col}")

    # Créer le dossier de sortie si nécessaire
    os.makedirs(output_dir, exist_ok=True)

    # Tracer le graphique
    plt.figure(figsize=(10, 6))
    for k_value in rounds:
        subset = df[df["k"] == k_value].sort_values(by="WEP")
        plt.plot(
            subset["WEP"],
            subset["Result"],
            marker='o',
            label=f"Round k = {k_value}"
        )

    plt.xlabel("WEP")
    plt.ylabel("CPS-HITL Probability (%)")
    #plt.title(f"Relation Between WEP et Result pour k = {', '.join(map(str, rounds))}")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    # Déterminer le nom du fichier image à partir du CSV
    base_name = os.path.splitext(os.path.basename(csv_file_path))[0]
    output_path = os.path.join(output_dir, f"{base_name}reliabilityWEPH.png")

    # Sauvegarder et fermer la figure
    plt.savefig(output_path)
    plt.close()

    print(f"Image générée pour {csv_file_path} → {output_path}")
    return output_path



csv_files = ["data1.csv", "data2.csv"]

for file in csv_files:
    generate_reliabilityWEPH_image(file)
