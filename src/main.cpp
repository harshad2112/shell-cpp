#include <filesystem>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>

std::string getPath(std::string command)
{
    std::string pathEnv = std::getenv("PATH");

    std::stringstream ss(pathEnv);
    std::string path;
    while(!ss.eof())
    {
        getline(ss, path, ':');
        if(std::filesystem::exists(path + "/" + command))
            return path + "/" + command;
    }
    return "";
}

std::vector<std::string> parseCommand(std::istringstream &input)
{
    std::vector<std::string> tokens;
    while(input >> std::ws)
    {
        std::string token;
        char firstChar = input.peek();
        if(firstChar == '\'' or firstChar == '"')
        {
            char quote = input.get();
            std::getline(input, token, quote);
        }
        else
        {
            input >> token;
        }
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
  // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    
    std::vector<std::string> supportedCommands = {"type", "echo", "exit", "pwd", "cd"};

    while(true)
    {
        std::cout << "$ ";

        std::string input;
        std::getline(std::cin, input);
        std::string command;
        std::istringstream stream(input);
        std::getline(stream, command, ' ');
        std::vector<std::string> args = parseCommand(stream);
        if(command == "exit")
        {
            return std::stoi(args[0]);
        }
        else if(command == "echo")
        {
            for(auto arg: args)
            {
                std::cout<<arg<<" ";
            }
            if(args.size())
                std::cout<<'\n';
        }
        else if(command == "type")
        {
            for(auto arg: args)
            {
                bool shellCommand = false;
                for(auto supCommand: supportedCommands)
                {
                    if(supCommand == arg)
                    {
                        shellCommand = true;
                        break;
                    }
                }
                if(shellCommand)
                {
                    std::cout<<arg<<" is a shell builtin\n";
                }
                else
                {   
                    std::string path = getPath(arg);
                    if(!path.empty())
                    {
                        std::cout<<arg<<" is "<<path<<'\n';
                    }
                    else
                    {
                        std::cout<<arg<<": not found\n";   
                    }
                }
            }
        }
        else if(command == "pwd")
        {
            std::string pwd = std::filesystem::current_path().string(); 
            std::cout<<pwd<<'\n';
        }
        else if(command == "cd")
        {
            if(args.size()==0 or args[0] == "~")
            {
                std::string home = std::getenv("HOME");
                args.resize(0);
                args.push_back(home);
            }
            if(args.size()!=1)
            {
                std::cerr<<"cd: too many arguments\n";
                continue;
            }
            std::filesystem::path newPath = args[0];
            if(!std::filesystem::exists(newPath))
            {
                std::cerr<<"cd: "<<args[0]<<": No such file or directory\n";
                continue;
            }
            std::filesystem::current_path(newPath);
        }
        else if(!getPath(command).empty())
        {
            std::system(input.c_str());
        }
        else 
        {
            std::cout<<command<<": command not found"<<'\n';
        }
    }
}
