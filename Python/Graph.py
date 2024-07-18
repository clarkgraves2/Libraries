"""
Climbing
2d matrix of heights, conduct BFS from S and end at E. Limitation, can only
move up or down one height difference (i.e. m->n but not m->o).

S will be represented by backtick because 96 == ord(`) and
E will be represented by open-curly because 123 = ord({)
allowing ascii characters to still function in the +/- 1 behavior required

Part 1:
Report how many steps from S to E.

Task is to write the breadth-first-search and then count the number of steps
"""
import queue from Queue

class Vertex:
    def __init__(self, x, y, value):
        self.x = x
        self.y = y
        self.value = value
        self.neighbors = []
        self.parent = None

    def __str__(self):
        return f"({self.x}, {self.y}, '{self.value}')"

    def __repr__(self):
        return f"({self.x}, {self.y}, '{self.value}')"


def main():
    graph = dict()
    start = None
    end = None

    # Create verticies and store a tuple of each location in nodes dict 
    with open("input.txt", "r") as file:
        for x, line in enumerate(file):
            for y, letter in enumerate(line.strip()):
                if letter == "S":
                    start = Vertex(x, y, "`")
                    graph[(x, y)] = start
                elif letter == "E":
                    end = Vertex(x, y, "{")
                    graph[(x, y)] = end
                else:
                    graph[(x, y)] = Vertex(x, y, letter)

    for key in graph.keys():
        current = graph[key]
        x = current.x
        y = current.y
        # x, y = key
        
        # if north exists in graph
        if (x - 1, y) in graph:
            neighbor = graph[(x - 1, y)]
            if ord(neighbor.value) <= (ord(current.value) + 1):
                current.neighbors.append(neighbor)

        # if east exists in graph
        if (x, y + 1) in graph:
            neighbor = graph[(x, y + 1)]
            if ord(neighbor.value) <= (ord(current.value) + 1):
                current.neighbors.append(neighbor)

        # if south exists in graph
        if (x + 1, y) in graph:
            neighbor = graph[(x + 1, y)]
            if ord(neighbor.value) <= (ord(current.value) + 1):
                current.neighbors.append(neighbor)

        # if west exists in graph
        if (x, y - 1) in graph:
            neighbor = graph[(x, y - 1)]
            if ord(neighbor.value) <= (ord(current.value) + 1):
                current.neighbors.append(neighbor)

    print(f"Start: {start}")
    print(f"Neighbors: {start.neighbors}")

    
    


if __name__ == '__main__':
    try:
        main()
    except (SystemExit, KeyboardInterrupt, GeneratorExit, Exception):
        from traceback import print_exc
        print_exc()


