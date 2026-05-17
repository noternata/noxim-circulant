#ifndef __NOXIMROUTING_CIRCULANT_H__
#define __NOXIMROUTING_CIRCULANT_H__

#include "RoutingAlgorithm.h"
#include "RoutingAlgorithms.h"
#include "../Router.h"

using namespace std;

class Routing_CIRCULANT : RoutingAlgorithm {
	public:
		vector<int> route(Router * router, const RouteData & routeData);

		static Routing_CIRCULANT * getInstance();

	private:
		Routing_CIRCULANT(){};
		~Routing_CIRCULANT(){};

		static Routing_CIRCULANT * routing_CIRCULANT;
		static RoutingAlgorithmsRegister routingAlgorithmsRegister;
};

#endif