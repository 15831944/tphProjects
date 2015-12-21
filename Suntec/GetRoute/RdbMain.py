#encoding=utf-8

from RdbDiGraph import RdbDiGraph

def main():
    G = RdbDiGraph()
    RdbDiGraph.bidirectional_dijkstra(G, -1, -1)
    return

if __name__ == "__main__":
    main()