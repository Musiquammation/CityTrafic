import re

filepath = "game/actions/action_character.cpp"


def parse_list(lines):
    types = {}
    in_list = False

    for i, line in enumerate(lines):
        if line.startswith("#define LIST"):
            in_list = True
            continue

        if in_list:
            stripped = line.strip()

            if not stripped:
                continue

            # stop si plus de backslash à la fin
            if not stripped.endswith("\\"):
                stripped = stripped.rstrip("\\")
                end = True
            else:
                stripped = stripped[:-1].strip()
                end = False

            # match fst(x), all(x), run(x)
            m = re.match(r"(fst|all|run)\((\w+)\)", stripped)
            if m:
                kind, name = m.groups()
                types[name] = kind

            if end:
                break

    return types


def parse_graphs(lines):
    graphs = {}

    content = "\n".join(lines)

    pattern = re.compile(r"graph\((.*?)\);", re.DOTALL)

    for match in pattern.finditer(content):
        block = match.group(1)

        parts = [p.strip() for p in block.split(",")]

        root = parts[0]
        children = [p.replace("&", "").strip() for p in parts[1:]]

        graphs[root] = children

    return graphs



def vertical_prefix(prefix):
    result = ""
    i = 0
    while i < len(prefix):
        chunk = prefix[i:i+4]
        if chunk == "│   ":
            result += "│   "
        else:
            result += "    "
        i += 4
    return result


def print_tree(node, graphs, types, prefix="", is_last=True, is_root=True, depth=0, state={"prev_depth": 0, "prev_was_last": False}):
    if not is_root:
        if state["prev_was_last"] and depth < state["prev_depth"]:
            print(vertical_prefix(prefix) + "|")

    if is_root:
        label = node
        if node in types and types[node] != "run":
            label += f" ({types[node]})"
        print(label)
    else:
        connector = "└── " if is_last else "├── "
        label = node
        if node in types and types[node] != "run":
            label += f" ({types[node]})"
        print(prefix + connector + label)

    state["prev_depth"] = depth
    state["prev_was_last"] = is_last

    if node not in graphs:
        return

    children = graphs[node]

    if is_root:
        new_prefix = ""
    else:
        new_prefix = prefix + ("    " if is_last else "│   ")

    for i, child in enumerate(children):
        last = i == len(children) - 1
        print_tree(child, graphs, types, new_prefix, last, False, depth + 1, state)



def main():
    with open(filepath, "r") as f:
        lines = f.readlines()

    types = parse_list(lines)
    graphs = parse_graphs(lines)

    root = "result"

    print_tree(root, graphs, types)


if __name__ == "__main__":
    main()