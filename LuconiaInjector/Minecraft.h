#pragma once

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <string>

class Minecraft
{
public:
	static std::optional<winrt::Windows::ApplicationModel::Package> getMinecraftPackage() {
		winrt::Windows::Management::Deployment::PackageManager manager;
		winrt::Windows::Foundation::Collections::IIterator<winrt::Windows::ApplicationModel::Package> packages = manager.FindPackagesForUser(winrt::hstring()).First();

		do
		{
			winrt::Windows::ApplicationModel::Package package = packages.Current();
			if (package.Id().Name() == L"Microsoft.MinecraftUWP") {
				return package;
			}
		} while (packages.MoveNext());
		return std::optional<winrt::Windows::ApplicationModel::Package>();
	}

	static std::string getVersion()
	{
		auto package = getMinecraftPackage();
		if (package.has_value())
		{
			winrt::Windows::ApplicationModel::PackageVersion version = package->Id().Version();
			return std::to_string(version.Major) + '.' + std::to_string(version.Minor) + '.' + std::to_string(version.Build > 0 ? version.Build / 100 : 0);
		}
	}
};