
struct game_offscreen_buffer{
	void* Memory;
	int Width;
	int Height;
	int Pitch;
};


void GameUpdateAndRender(game_offscreen_buffer* Buffer);


