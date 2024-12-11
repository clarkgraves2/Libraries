
class AVLNode:
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None
        self.height = 1

class AVLTree:
    def __init__(self):
        self.root = None

    def height(self, node):
        return node.height if node else 0

    def balance(self, node):
        return self.height(node.left) - self.height(node.right) if node else 0

    def update_height(self, node):
        if node:
            node.height = 1 + max(self.height(node.left), self.height(node.right))

    def right_rotate(self, y):
        x = y.left
        T2 = x.right
        x.right = y
        y.left = T2
        self.update_height(y)
        self.update_height(x)
        return x

    def left_rotate(self, x):
        y = x.right
        T2 = y.left
        y.left = x
        x.right = T2
        self.update_height(x)
        self.update_height(y)
        return y

    def insert(self, root, data):
        if not root:
            return AVLNode(data)
        if data < root.data:
            root.left = self.insert(root.left, data)
        else:
            root.right = self.insert(root.right, data)

        self.update_height(root)
        return self.balance_node(root, data)

    def balance_node(self, root, data):
        balance = self.balance(root)
        # Left Left Case
        if balance > 1 and data < root.left.data:
            return self.right_rotate(root)
        # Right Right Case
        if balance < -1 and data > root.right.data:
            return self.left_rotate(root)
        # Left Right Case
        if balance > 1 and data > root.left.data:
            root.left = self.left_rotate(root.left)
            return self.right_rotate(root)
        # Right Left Case
        if balance < -1 and data < root.right.data:
            root.right = self.right_rotate(root.right)
            return self.left_rotate(root)
        return root

    def insert_value(self, data):
        self.root = self.insert(self.root, data)

    def min_value_node(self, node):
        current = node
        while current.left:
            current = current.left
        return current

    def delete_node(self, root, data):
        if not root:
            return root
        if data < root.data:
            root.left = self.delete_node(root.left, data)
        elif data > root.data:
            root.right = self.delete_node(root.right, data)
        else:
            if not root.left:
                return root.right
            elif not root.right:
                return root.left
            temp = self.min_value_node(root.right)
            root.data = temp.data
            root.right = self.delete_node(root.right, temp.data)

        self.update_height(root)
        return self.balance_node(root, data)

    def delete_value(self, data):
        self.root = self.delete_node(self.root, data)

    def search(self, data):
        return self._search_recursive(self.root, data)

    def _search_recursive(self, node, data):
        if not node or node.data == data:
            return node
        if data < node.data:
            return self._search_recursive(node.left, data)
        return self._search_recursive(node.right, data)

    def inorder_traversal(self):
        return self._inorder_recursive(self.root)

    def _inorder_recursive(self, node):
        if not node:
            return []
        return (self._inorder_recursive(node.left) +
                [node.data] +
                self._inorder_recursive(node.right))

    def preorder_traversal(self):
        return self._preorder_recursive(self.root)

    def _preorder_recursive(self, node):
        if not node:
            return []
        return ([node.data] +
                self._preorder_recursive(node.left) +
                self._preorder_recursive(node.right))

    def postorder_traversal(self):
        return self._postorder_recursive(self.root)

    def _postorder_recursive(self, node):
        if not node:
            return []
        return (self._postorder_recursive(node.left) +
                self._postorder_recursive(node.right) +
                [node.data])

    def tree_height(self):
        return self.height(self.root)

    def lowest_common_ancestor(self, node, n1, n2):
        if not node:
            return None
        if node.data > n1 and node.data > n2:
            return self.lowest_common_ancestor(node.left, n1, n2)
        if node.data < n1 and node.data < n2:
            return self.lowest_common_ancestor(node.right, n1, n2)
        return node

    def find_lca(self, n1, n2):
        return self.lowest_common_ancestor(self.root, n1, n2)

    