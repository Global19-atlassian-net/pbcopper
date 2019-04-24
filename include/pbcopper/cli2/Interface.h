// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_INTERFACE_H
#define PBCOPPER_CLI_v2_INTERFACE_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/PbcopperConfig.h>
#include <pbcopper/cli2/Option.h>
#include <pbcopper/cli2/OptionGroup.h>
#include <pbcopper/cli2/PositionalArgument.h>
#include <pbcopper/cli2/Results.h>
#include <pbcopper/cli2/internal/InterfaceData.h>

namespace PacBio {
namespace CLI_v2 {

///
/// The main entry point for defining an application's CLI. The Interface provides
/// the info needed for printing help/version and defines all options and positional
/// arguments.
///
class Interface
{
public:
    Interface(std::string name, std::string description = std::string{},
              std::string version = std::string{});

    Interface(const Interface&);
    Interface(Interface&&);
    Interface& operator=(const Interface&);
    Interface& operator=(Interface&&);
    ~Interface();

public:
    ///
    /// Add an option to the interface.
    ///
    /// The option will be added to the default group.
    ///
    Interface& AddOption(const Option& option);

    ///
    /// Add options to the interface.
    ///
    /// The options will be added to the default group.
    ///
    Interface& AddOptions(const std::vector<Option>& options);

    ///
    /// Add an option group to the interface.
    ///
    Interface& AddOptionGroup(const std::string& title, const std::vector<Option>& options);

    ///
    /// Add an option group to the interface.
    ///
    Interface& AddOptionGroup(const OptionGroup& group);

    ///
    /// Add an positional argument to the interface.
    ///
    /// Positional arguments will be handled in the order they are registered.
    ///
    Interface& AddPositionalArgument(const PositionalArgument& posArg);

    ///
    /// Add positional arguments to the interface.
    ///
    /// Positional arguments will be handled in the order they are registered.
    ///
    Interface& AddPositionalArguments(const std::vector<PositionalArgument>& posArgs);

    ///
    /// Set application example.
    ///
    /// \note Currently only used when part of multi-tool interfaces.
    ///
    Interface& Example(std::string example);

public:
    ///
    /// \return application's description
    ///
    const std::string& ApplicationDescription() const;

    ///
    /// \return application's name
    ///
    const std::string& ApplicationName() const;

    ///
    /// \return application's version
    ///
    const std::string& ApplicationVersion() const;

    ///
    /// \return application example
    ///
    /// \note Currently only used when part of multi-tool interfaces.
    ///
    const std::string& Example() const;

    /// \internal
    /// \return "flattened" list of all registered options (grouping is ignored)
    ///
    std::vector<internal::OptionData> Options() const;

    /// \internal
    /// \return all registered option groups
    ///
    const std::vector<internal::OptionGroupData>& OptionGroups() const;

    /// \internal
    /// \return all registered positional arguments
    ///
    const std::vector<internal::PositionalArgumentData>& PositionalArguments() const;

public:
    /// \internal
    /// \return (translated) help option.
    ///
    const internal::OptionData& HelpOption() const;

    /// \internal
    /// \return (translated) log file option.
    ///
    const internal::OptionData& LogFileOption() const;

    /// \internal
    /// \return (translated) log level option.
    ///
    const internal::OptionData& LogLevelOption() const;

    /// \internal
    /// \return (translated) version option.
    ///
    const internal::OptionData& VersionOption() const;

public:
    ///
    /// Initializes the results object that will be populated from the command line
    /// or resolved tool contract
    ///
    Results MakeDefaultResults() const;

private:
    internal::InterfaceData data_;
};

}  // namespace CLI_v2
}  // namespace PacBio

#endif  // PBCOPPER_CLI_v2_INTERFACE_H