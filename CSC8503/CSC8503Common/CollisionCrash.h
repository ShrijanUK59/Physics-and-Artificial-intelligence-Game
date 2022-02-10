#pragma once
#include "TileGround.h"

namespace NCL {
	namespace CSC8503 {
		class CollisionCrash : public TileGround {
		public:
			CollisionCrash() {
				elasticity = 10.0f;		 
				name = "Jumping Tile"; 
			}
		};
	}
}