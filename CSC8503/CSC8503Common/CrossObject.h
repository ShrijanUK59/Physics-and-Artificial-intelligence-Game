#pragma once
#include "TileGround.h"
namespace NCL {
	namespace CSC8503 {
		class CrossObject : public TileGround {
		public:
			CrossObject() {
				friction = 0.0f;		 
				name = "Tile";
			}
		};
	}
}