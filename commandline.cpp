#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>  // Add this for std::istringstream
#include <iterator> // Add this for std::istream_iterator

 
#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 100
 
void execute_command(const std::string& token);
void batch_mode(std::istream& batch_stream);
void end_execution();
 
int main(int argc, char *argv[]) {
    if (argc == 2) {
        // Batch mode
        std::ifstream batch_file(argv[1]);
        if (!batch_file.is_open()) {
            perror("Error opening batch file");
            exit(EXIT_FAILURE);
        }
 
        batch_mode(batch_file);
        batch_file.close();
    } else {
        // Interactive mode
        std::string input;
        while (true) {
            std::cout << "$lopeShell: "; // Change this prompt as needed
            std::getline(std::cin, input);
 
            // Tokenize input based on semicolons
            std::vector<std::string> tokens;
            size_t pos = 0;
            while ((pos = input.find(';')) != std::string::npos) {
                tokens.push_back(input.substr(0, pos));
                input.erase(0, pos + 1);
            }
            tokens.push_back(input);
 
            // Execute each command
            for (const auto& token : tokens) {
                execute_command(token);
            }
        }
    }
 
    return 0;
}

// Add this function to handle the cd command
bool change_directory(const std::string& path) {
    if (chdir(path.c_str()) == 0) {
        return true; // Successfully changed directory
    } else {
        perror("cd failed");
        return false; // Failed to change directory
    }
}

void execute_command(const std::string& token) {
    // Split the token into words to check for the cd command
    std::istringstream iss(token);
    std::vector<std::string> args((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

    // Handle the cd command separately
    if (!args.empty() && args[0] == "cd") {
        if (args.size() > 1) {
            change_directory(args[1]);
        } else {
            std::cerr << "cd command requires a path argument" << std::endl;
        }
        return; // Don't proceed with forking
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Convert args to char* array for execvp
        char* argv[args.size() + 1];
        for (size_t i = 0; i < args.size(); ++i) {
            argv[i] = const_cast<char*>(args[i].c_str());
        }
        argv[args.size()] = NULL;

        if (execvp(argv[0], argv) == -1) {
            perror("Command execution failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        waitpid(pid, NULL, 0);
    }
}

void batch_mode(std::istream& batch_stream) {
    std::string line;
 
    while (std::getline(batch_stream, line)) {
        // Display and execute each line in the batch file
        std::cout << "$lopeShell: " << line << std::endl;
 
        // Tokenize input based on semicolons
        std::vector<std::string> tokens;
        size_t pos = 0;
        while ((pos = line.find(';')) != std::string::npos) {
            tokens.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        tokens.push_back(line);
 
        // Execute each command
        for (const auto& token : tokens) {
            execute_command(token);
        }
    }
 
    // Wait for all child processes to complete before returning to prompt
    while (wait(NULL) > 0)
        ;
}
 
void end_execution() {
    // You can implement any cleanup or additional functionality here
    exit(EXIT_SUCCESS);
}
