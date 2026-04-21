#!/usr/bin/env python3
import json
import random
import sys

def generate_test(num_points: int, seed: int = 42, coord_max: int = 100):
    """
    Генерирует JSON-строку с num_points терминалами.
    Координаты x, y — случайные целые числа в диапазоне [0, coord_max].
    """
    random.seed(seed)
    nodes = []
    for i in range(1, num_points + 1):
        node = {
            "name": f"p{i}",
            "x": random.randint(0, coord_max),
            "y": random.randint(0, coord_max),
            "id": i,
            "type": "t"
        }
        nodes.append(node)
    data = {"node": nodes, "edge": []}
    return json.dumps(data, indent=4)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python generate_test.py <num_points> [seed] [max_coord]")
        sys.exit(1)
    
    num_points = int(sys.argv[1])
    seed = int(sys.argv[2]) if len(sys.argv) > 2 else 42
    max_coord = int(sys.argv[3]) if len(sys.argv) > 3 else 100

    json_str = generate_test(num_points, seed, max_coord)
    # Сохраняем в файл
    filename = f"test_{num_points}.json"
    with open(filename, "w") as f:
        f.write(json_str)
    print(f"Generated {filename} with {num_points} points.")