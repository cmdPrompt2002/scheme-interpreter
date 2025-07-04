(define bst-from-list
  (lambda (lst)
    (bst-helper ((car lst) '() '()) lst)))

(define bst-helper
  (lambda (bst lst)
    (if (null? lst)
	bst
	(bst-helper (insert bst (car lst)) (cdr lst))))
	;append bst-from-lst (car lst)

