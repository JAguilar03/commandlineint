// Created by Jacob Aguilar and David Smical

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <iterator>
#include <cstdlib> 
#include <sys/stat.h> 
#include <cerrno> 

// Function declarations
void execute_command(const std::string& input);
void batch_mode(std::istream& batch_stream);
bool change_directory(const std::string& path);
void execute_piped_command(const std::vector<std::string>& cmd1, const std::vector<std::string>& cmd2);

int main(int argc, char *argv[]) {
    // Check if batch mode is enabled by passing a file as an argument
    if (argc == 2) {
        std::ifstream batch_file(argv[1]);
        if (!batch_file.is_open()) {
            perror("Error opening batch file");
            exit(EXIT_FAILURE);
        }
        batch_mode(batch_file);
        batch_file.close();
        std::cout << "Finished executing batch file. Entering interactive mode..." << std::endl;
    }

    // Interactive shell mode
    std::cout << "Entering interactive mode. Type 'exit' to quit." << std::endl;
    std::string input;
    while (true) {
        std::cout << "$lopeShell: ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        execute_command(input);
    }
    return 0;
}

// Function to display the first 'numLines' lines of a file
void head_command(const std::string& filename, int numLines = 10) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    
    std::string line;
    int lineCount = 0;
    while (lineCount < numLines && std::getline(file, line)) {
        std::cout << line << std::endl;
        ++lineCount;
    }
    file.close();
}

// Function to display the last 'numLines' lines of a file
void tail_command(const std::string& filename, int numLines = 10) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
        if (lines.size() > numLines) {
            lines.erase(lines.begin());
        }
    }
    
    for (const auto& l : lines) {
        std::cout << l << std::endl;
    }
    file.close();
}

// Function to change the current working directory
bool change_directory(const std::string& path) {
    if (chdir(path.c_str()) == 0) {
        return true;
    } else {
        perror("cd failed");
        return false;
    }
}

// Function to check if a given path is a directory
bool is_directory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) return false;
    return S_ISDIR(statbuf.st_mode);
}

// Function to move or rename a file
bool move_file(const std::string& source, const std::string& destination) {
    std::string final_destination = destination;
    
    // If the destination is a directory, append the filename
    if (is_directory(destination)) {
        size_t pos = source.find_last_of("/\\");
        std::string filename = (pos == std::string::npos) ? source : source.substr(pos + 1);
        final_destination = destination + (destination.back() == '/' ? "" : "/") + filename;
    }
    
    // Rename the file (move operation)
    if (rename(source.c_str(), final_destination.c_str()) != 0) {
        std::cerr << "Error moving/renaming file: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

// Function to execute a given command
void execute_command(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
    if (tokens.empty()) return;

    if (tokens[0] == "cd") {
        if (tokens.size() < 2) std::cerr << "cd: missing operand" << std::endl;
        else change_directory(tokens[1]);
    } else if (tokens[0] == "pwd") {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) std::cout << cwd << std::endl;
        else perror("pwd failed");
    } else if (tokens[0] == "echo") {
        for (size_t i = 1; i < tokens.size(); ++i) std::cout << tokens[i] << " ";
        std::cout << std::endl;
    } else if (tokens[0] == "procs") {
        system("ps -e");
    } else {
        int pid = fork();
        if (pid == 0) {
            std::vector<char*> args;
            for (auto& arg : tokens) args.push_back(&arg[0]);
            args.push_back(nullptr);
            execvp(args[0], args.data());
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            waitpid(pid, nullptr, 0);
        } else {
            perror("fork");
        }
    }
}

// Function to handle piped commands
void execute_piped_command(const std::vector<std::string>& cmd1, const std::vector<std::string>& cmd2) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        execvp(cmd1[0].c_str(), (char* const*)cmd1.data());
        perror("execvp cmd1");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        execvp(cmd2[0].c_str(), (char* const*)cmd2.data());
        perror("execvp cmd2");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

// Function to execute commands in batch mode
void batch_mode(std::istream& batch_stream) {
    std::string line;
    while (std::getline(batch_stream, line)) {
        if (line == "exit") {
            exit(EXIT_SUCCESS);
        }
        execute_command(line);
    }
}
