                {"breadth-limit",   3,    "N",   "", false, "Specify the breadth limit used for the topological ordering of strongly connected components."},
                {"depth-limit",     4,    "N",   "", false, "Specify the depth limit used for the topological ordering of strongly connected components."},
                {"lookahead",       5,    "N",   "", false, "Specify the lookahead used for the topological ordering of strongly connected components."}
    
    if (globalConfig.has("breadth-limit")) {
        int limit = std::stoi(globalConfig.get("breadth-limit"));
        if (limit <= 0)
            fail("error: breadth limit must be 1 or more");
        TopologicallySortedSCCGraph::BREADTH_LIMIT = limit;
     }
     if (globalConfig.has("depth-limit")) {
        int limit = std::stoi(globalConfig.get("depth-limit"));
        if (limit <= 0)
            fail("error: depth limit must be 1 or more");
        TopologicallySortedSCCGraph::DEPTH_LIMIT = limit;
     }
     if (globalConfig.has("lookahead")) {
        if (globalConfig.has("breadth-limit") || globalConfig.has("depth-limit"))
            fail("error: only one of either lookahead or depth-limit and breadth-limit may be specified");
        int lookahead = std::stoi(globalConfig.get("lookahead"));
        if (lookahead <= 0)
            fail("error: lookahead must be 1 or more");
        TopologicallySortedSCCGraph::LOOKAHEAD = lookahead;
     }
