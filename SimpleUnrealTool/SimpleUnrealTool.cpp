#include <iostream>
#include <fstream>
#include "json.hpp"
#include <cstdlib>

using namespace std;
using json = nlohmann::json;

namespace fs = std::filesystem;

bool IsUnrealFromSource(const std::string& enginePath) {
    fs::path engineDirectory(enginePath);
    
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
    
    if (IsUnrealFromSource(ProjectFilePath))
    {
        std::cout << "Unreal Engine est from Source" << std::endl;
    }
    else
    {
        std::cout << "Unreal Engine n'est pas From Source" << std::endl;
    }
}

std::string GetProjectName(const std::string& projectFilePath) {
    std::ifstream file(projectFilePath);
    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier .uproject." << std::endl;
        return "";
    }

    nlohmann::json projectJson;
    file >> projectJson;
    
    std::string projectName = projectJson["Modules"][0].value("Name", "");
    return projectName.empty() ? projectFilePath.substr(projectFilePath.find_last_of("\\") + 1, projectFilePath.find_last_of(".") - projectFilePath.find_last_of("\\") - 1) : projectName;
}

void BuildProject(const std::string& projectFilePath) {
    std::string projectName = GetProjectName(projectFilePath);
    if (projectName.empty()) {
        std::cerr << "Impossible de récupérer le nom du projet." << std::endl;
        return;
    }
    std::string buildPath = ".\\Engine\\Build\\BatchFiles\\Build.bat";
    std::string command = buildPath + " " + projectName + " Development Win64 " + projectFilePath + " -waitmutex -progress -NoHotReloadFromIDE";
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Échec de la compilation.\n";
    } else {
        std::cout << "Compilation réussie.\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage : MyTool [UPROJECT_PATH] [COMMAND] [OPTION]\n";
        return 1;
    }

    std::string projectPath = argv[1];
    std::string command = argv[2];

    if (command == "show-infos") {
        ShowProjectInfo(projectPath);
    } else if (command == "build") {
        BuildProject(projectPath);
    } else {
        std::cerr << "Commande inconnue.\n";
    }

    return 0;
}

