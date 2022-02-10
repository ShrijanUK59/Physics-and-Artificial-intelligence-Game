#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Plane.h"

namespace NCL {
	namespace Maths {
		struct RayCollision {
			void* node;			
			Vector3		collidedAt;		
			float		rayDistance;

			RayCollision() {
				node = nullptr;
				rayDistance = FLT_MAX;
			}
		};

		class Ray {
		public:
			Ray(Vector3 position, Vector3 direction) {
				this->position = position;
				this->direction = direction;
			}
			~Ray(void) {}

			Vector3 GetPosition() const { return position; }
			void SetPosition(Vector3 val) { position = val; }
			Vector3 GetDirection() const { return direction; }
			void SetDirection(Vector3 val) { direction = val; }

		protected:
			Vector3 position;	
			Vector3 direction;	
		};
	}
}