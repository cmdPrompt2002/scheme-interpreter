(letrec ((x
	  (lambda (n)
	    (if (equal? 1 n)
                1
                (* n (x (- n 1)))) ))) 
        (x 5))
