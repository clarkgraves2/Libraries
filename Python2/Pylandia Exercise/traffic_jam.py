import json
from heapq import heappush, heappop
import math
import itertools

class Vertex:
    def __init__(self, name):
        self.visited = False
        self.name = name
        self.connections = {}
    
    def add_connections(self, destination_street, travel_time):
        self.connections[destination_street] = travel_time

class Graph:
    def __init__(self):
        self.vertices = {}
    
    def add_vertex(self, name):
        if name not in self.vertices:
            new_vertex = Vertex(name)
            self.vertices[name] = new_vertex
        return self.vertices[name]
    
    def add_edge(self, from_name, to_name, travel_time):
        from_vertex = self.add_vertex(from_name)
        to_vertex = self.add_vertex(to_name)
        from_vertex.add_connections(to_vertex, travel_time)

class City_Map:
    def __init__(self):
        self.time = {}
    
    def add_times(self, time_period):
        new_graph = Graph()
        self.time[time_period] = new_graph
        return new_graph

class PriorityQueue:
    def __init__(self):
        self.elements = []
        self.counter = itertools.count()
    
    def empty(self):
        return len(self.elements) == 0
    
    def put(self, item, priority):
        count = next(self.counter)
        heappush(self.elements, (priority, count, item))
    
    def get(self):
        return heappop(self.elements)[2]

def dijkstra(graph, start_vertex, end_vertex):
    distances = {vertex: math.inf for vertex in graph.vertices.values()}
    distances[start_vertex] = 0
    pq = PriorityQueue()
    pq.put(start_vertex, 0)
    previous = {vertex: None for vertex in graph.vertices.values()}

    while not pq.empty():
        current_vertex = pq.get()

        if current_vertex == end_vertex:
            path = []
            while current_vertex:
                path.append(current_vertex.name)
                current_vertex = previous[current_vertex]
            return path[::-1], distances[end_vertex]

        for neighbor, weight in current_vertex.connections.items():
            distance = distances[current_vertex] + weight
            if distance < distances[neighbor]:
                distances[neighbor] = distance
                previous[neighbor] = current_vertex
                pq.put(neighbor, distance)

    return None, math.inf  # No path found

def find_shortest_path(city_map, start_street, end_street, time_period):
    if time_period not in city_map.time:
        return None, math.inf
    
    graph = city_map.time[time_period]
    if start_street not in graph.vertices or end_street not in graph.vertices:
        return None, math.inf
    
    start_vertex = graph.vertices[start_street]
    end_vertex = graph.vertices[end_street]
    
    return dijkstra(graph, start_vertex, end_vertex)

def load_city_data(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
    
    city_map = City_Map()
    
    for time_period, streets in data.items():
        graph = city_map.add_times(time_period)
        
        for from_street, connections in streets.items():
            for to_street, travel_time in connections.items():
                graph.add_edge(from_street, to_street, travel_time)
    
    return city_map

# Usage
city_map = load_city_data('traffic_routes.json')

# Example: Find the shortest path between two streets at a specific time
start_street = "Snake Loop"
end_street = "Debug Drive"
time_period = "0800"

path, total_time = find_shortest_path(city_map, start_street, end_street, time_period)

if path:
    print(f"Shortest path from {start_street} to {end_street} at {time_period}:")
    print(" -> ".join(path))
    print(f"Total travel time: {total_time} minutes")
else:
    print(f"No path found from {start_street} to {end_street} at {time_period}")

