#pragma once
#include "CubeObject.h"

namespace NCL {
	namespace CSC8503 {
		class SpinningTile : public CubeObject {
		public:
			SpinningTile() { 
				invMass = 0.0f;
				elasticity = 10.0f;
				name = "Spinning Tile";
			}
		};
	}
}