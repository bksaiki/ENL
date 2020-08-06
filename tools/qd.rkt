#lang racket

(require math/bigfloat)

(define (real->qd x)
  (define x* (if (bigfloat? x) x (bf x)))
  (for/fold ([qd '()] [x x*] #:result (reverse qd))
            ([i (in-range 4)])
    (let ([y (bigfloat->flonum x)])
    (values (cons y qd) (bf- x (bf y))))))

(define (batch-real->qd vals descrs)
  (for ([val vals] [desc descrs])
    (apply printf "~a: ~a + ~a + ~a + ~a\n" desc (real->qd val))))

(module+ main
  (define mode #f)
  (parameterize ([bf-precision 512])
   (command-line
    #:program "qd.rkt"
    #:once-each
    ["--constants" "Returns the quad-double values of C math constants"
     (set! mode 'constants)]
    ["--eval" value "Converts a real value to a quad-double number"
     (set! mode 'eval)]
    #:args ([head #f] . rest)
    (match mode
     ['eval
      (define vals (cons head rest))
      (if (andmap string? vals)
          (batch-real->qd vals vals)
          (printf "Expected values to convert\n"))]
     ['constants
      (define constants
        (list
          (cons "E" (bfexp 1.bf))
          (cons "LOG2E" (bflog2 (bfexp 1.bf)))
          (cons "LOG2E" (bflog10 (bfexp 1.bf)))
          (cons "LN2" log2.bf)
          (cons "LN10" (bflog 10.bf))
          (cons "PI" pi.bf)
          (cons "PI_2" (bf/ pi.bf 2.bf))
          (cons "PI_4" (bf/ pi.bf 4.bf))
          (cons "1_PI" (bf/ 1.bf pi.bf))
          (cons "2_PI" (bf/ 2.bf pi.bf))
          (cons "2_SQRTPI" (bf/ 2.bf (bfsqrt pi.bf)))
          (cons "SQRT2" (bfsqrt 2.bf))
          (cons "SQRT1_2" (bfsqrt (bf/ 1.bf 2.bf)))))
      (batch-real->qd (map cdr constants) (map car constants))]  
     [_ 
      (printf "No mode specified\n")]))))