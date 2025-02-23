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

int main() {
  // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    
    std::vector<std::string> supportedCommands = {"type", "echo", "exit"};

    while(true)
    {
        std::cout << "$ ";

        std::string input;
        std::getline(std::cin, input);
        std::string command;
        std::vector<std::string> args;
        int start = 0, end = 0;
        while((end = input.find(' ', start)) != std::string::npos)
        {
            if(command == "")
                command = input.substr(start, end - start);
            else
                 args.push_back(input.substr(start, end - start));
            start = end + 1;
        }
        if(command == "")
            command = input.substr(start);
        else
            args.push_back(input.substr(start));
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
