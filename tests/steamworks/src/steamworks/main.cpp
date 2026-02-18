#include <crimson/renderer>
#include <iostream>
#include <steam/steam_api.h>

using namespace mstd;
using namespace ct;

class Game : public Context {
public:
	Game() : Context("steamworks", 0, nullptr) {

	}

	void init() override {
		auto f = SteamFriends();
		I32 friendCount = f->GetFriendCount(k_EFriendFlagAll);
		for (I32 i = 0; i < friendCount; ++i) {
			CSteamID id = f->GetFriendByIndex(i, k_EFriendFlagAll);
			std::cout << i << ": " << f->GetFriendPersonaName(id) << std::endl;
		}

		std::cout << "Index to dump?" << std::endl;
		I32 index;
		std::cin >> index;

		CSteamID id = f->GetFriendByIndex(index, k_EFriendFlagAll);
		std::cout << "Confirming " << f->GetFriendPersonaName(id) << std::endl;

		mstd::Bool x;
		std::cin >> x;

		if (x)
		f->ReplyToFriendMessage(id, "nwod");
	}

	void update() override {
		glClear(GL_COLOR_BUFFER_BIT);
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	void end() override {
	}
};

I32 main() {
	SteamAPI_Init();

	Game game;
	game.run();

	SteamAPI_Shutdown();

	return 0;
}
