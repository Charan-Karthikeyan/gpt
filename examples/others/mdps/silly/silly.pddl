(define (domain silly)
  (:model (:dynamics :non-deterministic) (:feedback :complete))
  (:objects state - :integer[0,5])

  (:action init
    :precondition (= state 0)
    :effect
      (:non-deterministic
         ((:set state 1))
         ((:set state 2)))
  )

  (:action finish
    :precondition (:not (= state 0))
    :effect
      (:set state (+ state 2))
  )

  (:action other)
)


(define (problem p1)
  (:domain silly)
  (:init (:set state 0))
  (:goal (:or (= state 3) (= state 4)))
)

