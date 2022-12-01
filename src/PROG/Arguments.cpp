#include "PROG/Arguments.hpp"

namespace KCT::util {

Arguments::Arguments(int argc, char* argv[], std::string prgInfo)
    : argc(argc)
    , argv(argv)
    , prgInfo(prgInfo)
{
    cliApp = std::make_shared<CLI::App>(prgInfo);
}

int Arguments::parse(bool helpOnError)
{
    this->defineArguments();
    try
    {
        int preVal, postVal;
        preVal = preParse();
        if(preVal != 0)
        {
            return preVal;
        }
        cliApp->parse(argc, argv);
        postVal = postParse();
        if(postVal != 0)
        {
            return postVal;
        }
    } catch(const CLI::CallForHelp& e)
    {
        cliApp->exit(e); // Prints help message
        return 1;
    } catch(const CLI::ParseError& e)
    {
        int exitcode = cliApp->exit(e);
        std::string cmd = io::xprintf("%s", argv[0]);
        for(int i = 1; i < argc; i++)
        {
            cmd = io::xprintf("%s %s", cmd.c_str(), argv[i]);
        }
        LOGE << io::xprintf("Parse error with exit code %d when parsing %s", exitcode, cmd.c_str());
        if(helpOnError)
        {
            std::cout << cliApp->help();
        }
        return -1;
    } catch(...)
    {
        LOGE << "Unknown exception catched";
        return -1;
    }
    return 0;
}

std::shared_ptr<CLI::App> Arguments::getCliApp() { return cliApp; }
void Arguments::registerOption(std::string optName, CLI::Option* opt)
{
    if(cliOptions.find(optName) != cliOptions.end())
    {
        std::string err
            = io::xprintf("Option with a name %s is already registered!", optName.c_str());
        LOGE << err;
        throw std::runtime_error(err);
    }
    cliOptions[optName] = opt;
}

void Arguments::registerOptionGroup(std::string optGroupName, CLI::Option_group* og)
{
    if(cliOptionGroups.find(optGroupName) != cliOptionGroups.end())
    {
        std::string err
            = io::xprintf("Option with a name %s is already registered!", optGroupName.c_str());
        LOGE << err;
        throw std::runtime_error(err);
    }
    cliOptionGroups[optGroupName] = og;
}

CLI::Option_group* Arguments::getRegisteredOptionGroup(std::string optGroupName)
{
    if(cliOptionGroups.find(optGroupName) == cliOptionGroups.end())
    {
        return nullptr;
    } else
    {
        return cliOptionGroups[optGroupName];
    }
}

CLI::Option* Arguments::getRegisteredOption(std::string optName)
{
    if(cliOptions.find(optName) == cliOptions.end())
    {
        return nullptr;
    } else
    {
        return cliOptions[optName];
    }
}

} // namespace KCT::util
