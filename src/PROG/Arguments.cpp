#include "PROG/Arguments.hpp"

namespace CTL::util {

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
        LOGE << io::xprintf("There was perse error with exit code %d catched.", exitcode);
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

} // namespace CTL::util
