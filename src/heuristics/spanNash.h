int runSpanningNashHeuristic(graphGroup g, const vector<journeyInfo>& journeyInfos) {

    int lowestSoFar = INT_MAX;
    int lowestStartVertex = -1;
    for(int startVertex = 0; startVertex < g.returnN(); startVertex++) {
        STGroup st;
        st.findMinSpanningTree(g.returnGraph(), startVertex);

        graphGroup spanningTree(st.returnMinSpanningTree(), journeyInfos);

        for(int i = 0; i < journeyInfos.size(); i++) {
            spanningTree.addJourneySP(i);
            spanningTree.refindSharedCosts();
        }

        int total = 0;
        for(int i = 0; i < journeyInfos.size(); i++) {
            total += spanningTree.returnSharedCost(i).value();
        }

        if(total < lowestSoFar) {
            lowestSoFar = total;
            lowestStartVertex = startVertex;
        }
    }

    STGroup st;
    st.findMinSpanningTree(g.returnGraph(), lowestStartVertex);

    graphGroup spanningTree(st.returnMinSpanningTree(), journeyInfos);

    for(int i = 0; i < journeyInfos.size(); i++) {
        spanningTree.addJourneySP(i);
        spanningTree.refindSharedCosts();
    }

    // Route the paths back on the original graph
    for(int i = 0; i < journeyInfos.size(); i++) {
        path p = spanningTree.returnSharedPath(i);
        g.addJourney(i, p);
    }

    bool nash = nashEquilibrium(g);

    dumpGraph(g, "SpanNash");

    int totalCost = 0;
    for(int i = 0; i < journeyInfos.size(); i++) {
        totalCost += g.returnSharedCost(i).value();
    }

    return totalCost;
}