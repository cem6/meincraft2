dependencies
	SDL3, 
	sonst alles mit drin

keybinds
	wasd, space, ctrl: move
	b: toggle capture mouse	
	e: togggle render mode line
	c: toggle 2d camera

TODO
	optimize culling: generation queue: move -> push chunks, sort
										frame -> update: pop and generate
	? cache chunkMap

	moving lightpos

	atlas with gaps
	antialiasing (window) und mipmap (texture)

	noise besser
	physik
	bauen

SPÄTER
	thread für inf worldgen 
