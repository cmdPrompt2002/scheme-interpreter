(import (rnrs))


(define curry3
  (lambda (f)
    (lambda (a)
      (lambda (b)
	(lambda (c)
	  (f a b c))))))

(define uncurry3
  (lambda (f)
    (lambda (a b c)
      (((f a) b) c))))

(define my-filter
  (lambda (func lst)
    (if (null? lst)
	'()
	(if (func (car lst))
	    (cons (car lst) (my-filter func (cdr lst)))
	    (my-filter func (cdr lst))))))

(define exists
  (lambda (func lst)
    (if (null? (my-filter func lst))
	#f
	#t)))


(define find-docs
  (lambda (val docs)
    (filter not-null? (map (extract val) docs))))

(define extract
  (lambda (val)
    (lambda (lst)
      (if (exists (is-val? val) (car (cdr lst)))
	  (car lst)
	  '()))))
		
(define is-val?
  (lambda (val)
    (lambda (item)
      (equal? val item))))

(define not-null?
  (lambda (item)
    (if (equal? '() item)
	#f
	#t)))

(define doclist '((cs (Welcome to the Carleton CS department))
                  (art (The department consists of two separate majors))
                  (geology (The geology department retains a spirit of exploration))
                  (president (Alison R Byerly is Carleton College’s 12th president))))


(fold-left cons '() '(1 2 3 4))

(define newUncurry
  (lambda (func)
    (lambda (a b c)
     (((func a) b) c))))
