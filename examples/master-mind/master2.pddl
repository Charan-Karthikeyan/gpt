;; Master Mind
;; ===========
;;
;; Variables:
;;
;;    di  - for digits in { 0 1 2 }
;;

(define (domain master2)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:objects d0 d1 d2 d3 - :integer[0,2])

  (:predicate (correct ?x0 ?x1 ?x2 ?x3 - :integer[0,2])
     (:and (= ?x0 d0) (= ?x1 d1) (= ?x2 d2) (= ?x3 d3))
  )

  (:predicate (hit ?x - :integer[0,2])
     (:or (= ?x d0) (= ?x d1) (= ?x d2) (= ?x d3))
  )

  (:action guess
     :parameters ?x0 ?x1 ?x2 ?x3 - :integer[0,2] 
     :observation (+ (:if (= ?x0 d0) 1 0)
                     (+ (:if (= ?x1 d1) 1 0)
                        (+ (:if (= ?x2 d2) 1 0) (:if (= ?x3 d3) 1 0))))
                  (+ (:if (hit ?x0) 1 0)
                     (+ (:if (hit ?x1) 1 0)
                        (+ (:if (hit ?x2) 1 0) (:if (hit ?x3) 1 0))))
  )
)

(define (problem p2)
  (:domain master2)
  (:init (:set d0 :in { 0 1 2 })
         (:set d1 :in { 0 1 2 })
         (:set d2 :in { 0 1 2 })
         (:set d3 :in { 0 1 2 })
  )
  (:goal :certainty)
)
