
(+ 1 2)

;;; if checks whether the condition inside the parentheses is true
;;; if true, execute the next line of code
;;; if false , execute the second line of code
(if (equal? 3 3) 
    9
    10)
;;; cond is like if, else if, else sequence
(cond ((equal? 3 4) (+ 3 8))
      ((equal? 1 8) 12)
      (else (* 6 3)))

(define add-one
  (lambda (x)
    (+ x 1)))

(define another-add-one add-one) ;Aliasing: make another variable name for the same procedure
(another-add-one 5)

(define sum
  (lambda (lst)
    (if (null? lst)
	0
	(+ (car lst) (sum (cdr lst))))))


(define keep-first-n
  (lambda (n lst)
    
    (cond ((or (< n 0) (equal? n 0) (> n (length lst))) '())
	  (else (cons (car lst) (keep-first-n (- n 1) (cdr lst)))))))

(define nth-again
  (lambda (lst n)
    (cond ((null? lst)  #f)
	  ((equal? n 0) (car lst))
	  ((< n 0) #f)
	  (else (nth-again (cdr lst) (- n 1))))))
