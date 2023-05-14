#pragma once

#include <discord_register.h>
#include <discord_rpc.h>
#include <windows.h>

class Discord
{
public:
	static void init()
	{
		DiscordEventHandlers handle;
		memset(&handle, 0, sizeof(handle));
		Discord_Initialize("1066805525540507728", &handle, 1, NULL);
	}
	//TODO: Finish this
	static void update()
	{
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));
		discordPresence.state = "In Launcher";
		discordPresence.details = "Idle";
		discordPresence.startTimestamp = time(0);
		discordPresence.largeImageKey = "luconia";
		discordPresence.largeImageText = "Luconia Client";
		discordPresence.button1Label = "Discord";
		discordPresence.button1Url = "https://discord.gg/luconia";
        Discord_UpdatePresence(&discordPresence);
	}
};