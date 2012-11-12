;; Master Mind
;; ===========
;;
;; Variables:
;;
;;    di  - for digits in { 0 1 2 }
;;

(define (domain master)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:objects d0 d1 d2 - :integer[0,2])

  (:predicate (correct ?x0 ?x1 ?x2 - :integer[0,2])
     (:and (= ?x0 d0) (= ?x1 d1) (= ?x2 d2))
  )

  (:predicate (hit ?x - :integer[0,2])
     (:or (= ?x d0) (= ?x d1) (= ?x d2))
  )

  (:action guess
     :parameters ?x0 ?x1 ?x2 - :integer[0,2] 
     :observation (+ (:if (= ?x0 d0) 1 0)
                     (+ (:if (= ?x1 d1) 1 0) (:if (= ?x2 d2) 1 0)))
                  (+ (:if (hit ?x0) 1 0)
                     (+ (:if (hit ?x1) 1 0) (:if (hit ?x2) 1 0)))
  )
)

(define (problem p1)
  (:domain master)
  (:init (:set d0 :in { 0 1 2})
         (:set d1 :in { 0 1 2})
         (:set d2 :in { 0 1 2})
  )
  (:goal :certainty)
)
