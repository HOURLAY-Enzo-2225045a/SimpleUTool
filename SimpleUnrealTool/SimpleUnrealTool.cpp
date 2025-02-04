#include <iostream>
#include <fstream>
#include "json.hpp"
#include <cstdlib>

using namespace std;
using json = nlohmann::json;

namespace fs = std::filesystem;

bool IsUnrealFromSource(const std::string& enginePath) {
    fs::path engineDirectory(enginePath);

    // Remonter de deux niveaux
    engineDirectory = engineDirectory.parent_path().parent_path().parent_path();
    
    fs::path solutionPath = engineDirectory / "UE5.sln";
    return fs::exists(solutionPath);
}

void ShowProjectInfo(const std::string& ProjectFilePath)
{
    std::ifstream file(ProjectFilePath);
    if (!file.is_open())
    {
        std::cerr << "Impossible d'ouvrir le fichier .uproject." << std::endl;
        return;
    }
    
    nlohmann::json projectJson;
    file >> projectJson;

    // Récupérer le nom du jeu avec un contrôle pour un cas vide
    std::string gameName = projectJson["Modules"][0].value("Name", "Nom non défini");

    // Récupérer la version d'Unreal
    std::string unrealVersion = projectJson.value("EngineAssociation", "Version non trouvée");

    // Affichage
    std::cout << "Nom du jeu : " << gameName << std::endl;
    std::cout << "Version UE : " << unrealVersion << std::endl;

    // Plugins utilisés
    std::cout << "Plugin(s):" << std::endl;
    for (const auto& plugin : projectJson["Plugins"])
    {
        std::cout << "- " << plugin["Name"] << std::endl;
    }

    // Vérification si Unreal est compilé from source
    if (IsUnrealFromSource(ProjectFilePath))
    {
        std::cout << "Unreal Engine est from Source" << std::endl;
    }
    else
    {
        std::cout << "Unreal Engine n'est pas From Source" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage : SimpleUnrealTool [UPROJECT_PATH] [COMMAND] [OPTION]\n";
        return 1;
    }

    std::string projectPath = argv[1];
    std::string command = argv[2];

    if (command == "show-infos") {
        ShowProjectInfo(projectPath);
    } else {
        std::cerr << "Commande inconnue.\n";
    }

    return 0;
}
