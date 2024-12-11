# extended_b_plus_tree.py

class BPlusNode:
    def __init__(self, leaf=False):
        self.leaf = leaf
        self.keys = []
        self.children = []
        self.next = None  # For leaf node linking

class BPlusTree:
    def __init__(self, t):
        self.root = BPlusNode(leaf=True)
        self.t = t  # Minimum degree (defines the range for number of keys)

    def search(self, k):
        return self._search_internal(self.root, k)

    def _search_internal(self, x, k):
        i = 0
        while i < len(x.keys) and k > x.keys[i]:
            i += 1
        if x.leaf:
            if i < len(x.keys) and x.keys[i] == k:
                return (x, i)
            return None
        else:
            return self._search_internal(x.children[i], k)

    def insert(self, k):
        root = self.root
        if len(root.keys) == (2 * self.t) - 1:
            new_root = BPlusNode()
            self.root = new_root
            new_root.children.insert(0, root)
            self._split_child(new_root, 0)
            self._insert_non_full(new_root, k)
        else:
            self._insert_non_full(root, k)

    def _insert_non_full(self, x, k):
        i = len(x.keys) - 1
        if x.leaf:
            x.keys.append(None)
            while i >= 0 and k < x.keys[i]:
                x.keys[i + 1] = x.keys[i]
                i -= 1
            x.keys[i + 1] = k
        else:
            while i >= 0 and k < x.keys[i]:
                i -= 1
            i += 1
            if len(x.children[i].keys) == (2 * self.t) - 1:
                self._split_child(x, i)
                if k > x.keys[i]:
                    i += 1
            self._insert_non_full(x.children[i], k)

    def _split_child(self, x, i):
        t = self.t
        y = x.children[i]
        z = BPlusNode(leaf=y.leaf)
        x.children.insert(i + 1, z)
        x.keys.insert(i, y.keys[t - 1])
        z.keys = y.keys[t:]
        y.keys = y.keys[:t]
        if not y.leaf:
            z.children = y.children[t:]
            y.children = y.children[:t]
        else:
            z.next = y.next
            y.next = z

    def delete(self, k):
        self._delete_internal(self.root, k)
        if len(self.root.keys) == 0 and not self.root.leaf:
            self.root = self.root.children[0]

    def _delete_internal(self, x, k):
        t = self.t
        i = 0
        while i < len(x.keys) and k > x.keys[i]:
            i += 1
        if x.leaf:
            if i < len(x.keys) and x.keys[i] == k:
                x.keys.pop(i)
            return
        if i < len(x.keys) and x.keys[i] == k:
            return self._delete_from_internal_node(x, k, i)
        elif len(x.children[i].keys) >= t:
            self._delete_internal(x.children[i], k)
        else:
            if i != 0 and i + 2 < len(x.children):
                if len(x.children[i - 1].keys) >= t:
                    self._borrow_from_prev(x, i)
                elif len(x.children[i + 1].keys) >= t:
                    self._borrow_from_next(x, i)
                else:
                    self._merge(x, i)
            elif i == 0:
                if len(x.children[i + 1].keys) >= t:
                    self._borrow_from_next(x, i)
                else:
                    self._merge(x, i)
            elif i + 1 == len(x.children):
                if len(x.children[i - 1].keys) >= t:
                    self._borrow_from_prev(x, i)
                else:
                    self._merge(x, i - 1)
            self._delete_internal(x.children[i], k)

    def _delete_from_internal_node(self, x, k, i):
        if x.leaf:
            if x.keys[i] == k:
                x.keys.pop(i)
            return
        if len(x.children[i].keys) >= self.t:
            x.keys[i] = self._get_pred(x, i)
            self._delete_internal(x.children[i], x.keys[i])
        elif len(x.children[i + 1].keys) >= self.t:
            x.keys[i] = self._get_succ(x, i)
            self._delete_internal(x.children[i + 1], x.keys[i])
        else:
            self._merge(x, i)
            self._delete_internal(x.children[i], k)

    def _get_pred(self, x, i):
        current = x.children[i]
        while not current.leaf:
            current = current.children[-1]
        return current.keys[-1]

    def _get_succ(self, x, i):
        current = x.children[i + 1]
        while not current.leaf:
            current = current.children[0]
        return current.keys[0]

    def _borrow_from_prev(self, x, i):
        child = x.children[i]
        sibling = x.children[i - 1]
        child.keys.insert(0, x.keys[i - 1])
        x.keys[i - 1] = sibling.keys.pop()
        if not child.leaf:
            child.children.insert(0, sibling.children.pop())

    def _borrow_from_next(self, x, i):
        child = x.children[i]
        sibling = x.children[i + 1]
        child.keys.append(x.keys[i])
        x.keys[i] = sibling.keys.pop(0)
        if not child.leaf:
            child.children.append(sibling.children.pop(0))

    def _merge(self, x, i):
        child = x.children[i]
        sibling = x.children[i + 1]
        child.keys.append(x.keys[i])
        child.keys.extend(sibling.keys)
        if not child.leaf:
            child.children.extend(sibling.children)
        x.keys.pop(i)
        x.children.pop(i + 1)
        child.next = sibling.next

    def range_query(self, start, end):
        result = []
        leaf = self._find_leaf(self.root, start)
        while leaf:
            for key in leaf.keys:
                if start <= key <= end:
                    result.append(key)
                elif key > end:
                    return result
            leaf = leaf.next
        return result

    def _find_leaf(self, x, k):
        if x.leaf:
            return x
        i = 0
        while i < len(x.keys) and k > x.keys[i]:
            i += 1
        return self._find_leaf(x.children[i], k)

    def print_tree(self):
        self._print_internal(self.root, 0)

    def _print_internal(self, x, level):
        print(f"Level {level}:", end=" ")
        print(x.keys)
        if not x.leaf:
            for i in range(len(x.children)):
                self._print_internal(x.children[i], level + 1)

