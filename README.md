# Object Labels

A replacement/recreation of 3D Text Labels, but with objects.
They were originally supposed to mimic the little price tags you see on buyable properties in Singleplayer, but the pricedown font sucks in SetDynamicObjectMaterialText.

You can choose to orientate the labels based on the position of players (fast updates) or the camera position of players (slow updates but looks better).

This include uses the Streamer Plugin. There is only one dynamic object created per label. While it is streamed in, this include simply modifies the PlayerObject rotation of that object (Streamer Plugin uses Player Objects).
