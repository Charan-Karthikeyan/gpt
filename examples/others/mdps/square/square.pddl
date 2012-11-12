(define (domain square)
  (:model (:dynamics :probabilistic) (:feedback :complete))
  (:external (:function above (:integer[1,100] :integer[1,10000]) :integer[1,10000])
             (:function below (:integer[1,100] :integer[1,10000]) :integer[1,10000])
             (:function left  (:integer[1,100] :integer[1,10000]) :integer[1,10000])
             (:function right (:integer[1,100] :integer[1,10000]) :integer[1,10000])
  )
  (:objects size - :integer[1,100]
            pos - :integer[1,10000])

  (:action go_up
    :effect (:probabilistic (0.75 (:set pos (above size pos))))
  )

  (:action go_down
    :effect (:probabilistic (0.75 (:set pos (below size pos))))
  )

  (:action go_right
    :effect (:probabilistic (0.75 (:set pos (right size pos))))
  )

  (:action go_left
    :effect (:probabilistic (0.75 (:set pos (left size pos))))
  )
)

(define (problem p4)
  (:domain square)
  (:init (:set size 4)
         (:set pos 1))
  (:goal (= pos 16)))

(define (problem p8)
  (:domain square)
  (:init (:set size 8)
         (:set pos 1))
  (:goal (= pos 64)))

