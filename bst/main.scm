

;@Param bst, a binary search tree. Checks whether the bst is valid. Returns the node if valid.
(define entry
  (lambda (bst)
    (if (and (treeChecker? bst) (not (null? bst)))
	(car bst)
	#f)))

;@param bst, a binary serach tree. If bst is valid then return true, otherwise false. A bst is valid if it is null or contains three entries where the last two are bsts.
(define treeChecker?
  (lambda (bst)
    (cond ((null? bst) #t)
	  ((not (list? bst)) #f)
	  ((not (equal? (length bst) 3)) #f)
	  ((and (list? (car (cdr bst))) (list? (car (cdr (cdr bst)))) (not (list? (car bst))))
	   (and (treeChecker? (car (cdr bst))) (treeChecker? (car (cdr (cdr bst)))) #t))
	  (else #f))))

;@Param bst, a binary search tree, checks whether the bst is valid. Returns the left subtree if valid.
(define left
    (lambda (bst)
        (if (and (treeChecker? bst) (not (null? bst)))
        (car (cdr bst))
        #f)))

;@Param bst,  a binary search tree, checks whether the bst is valid. Returns the right subtree if valid.
(define right
  (lambda (bst)
    (if (and (treeChecker? bst) (not (null? bst)))
	(car (cdr (cdr bst)))
	#f)))

;@Param elt (the node value), left-bst (left subtree), and right-bst (right subtree). Returns a singular bst containing the params.
(define make-bst
  (lambda (elt left-bst right-bst)
    (if (and (or (null? left-bst) (treeChecker? left-bst)) (or (null? right-bst) (treeChecker? right-bst)) (integer? elt))
	(list elt
	      left-bst
	      right-bst)
	#f)))

;@Param bst,  a binary search tree. Returns a list containing the bst elements in a preorder traversal of the tree. A preorder traversal returns elements in order of [top node, preorder traversal of left subtree, preorder traversal of right subtree]
(define preorder
  (lambda (bst)
    (if (null? bst)
	'()
	(append (cons (car bst) (preorder (left bst))) (preorder (right bst))))))

;@Param bst, a binary search tree. Returns a list containing the bst elements from smallest to largest.
(define inorder
  (lambda (bst)
  (if (null? bst)
      '()
      (append (append (inorder (left bst)) (list (car bst))) (inorder (right bst))))))

;@Param bst, a binary search tree. Returns a list containing the elements [postorder traversal of left subtree, postorder traveral of right subtree, node].
(define postorder
  (lambda (bst)
  (if (null? bst)
      '()
      (append (postorder (left bst)) (postorder (right bst)) (list (car bst))))))
	 
;@Param v, a new value to be inserted, and bst, a valid binary search tree. Inserts v in the correct spot of the bst and returns the newly formed bst. If v is already in bst, returns the identical bst.
(define insert
  (lambda (v bst)
    (cond ((null? bst) (list v '() '()))
	  ((equal? v (car bst)) bst)
	  ((< v (car bst)) (make-bst (car bst) (insert v (left bst)) (right bst)))
	  ((> v (car bst)) (make-bst (car bst) (left bst) (insert v (right bst))))
	  (else #f))))


