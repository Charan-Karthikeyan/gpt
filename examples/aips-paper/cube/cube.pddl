(define (domain cube)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:external (:function above (:integer[1,20] :integer[1,10000]) :integer[1,10000])
             (:function below (:integer[1,20] :integer[1,10000]) :integer[1,10000])
             (:function left (:integer[1,20] :integer[1,10000]) :integer[1,10000])
             (:function right (:integer[1,20] :integer[1,10000]) :integer[1,10000])
             (:function in (:integer[1,20] :integer[1,10000]) :integer[1,10000])
             (:function out (:integer[1,20] :integer[1,10000]) :integer[1,10000])
  )
  (:objects size - :integer[1,20]
            pos - :integer[1,10000])

  (:action go_up
    :effect (:set pos (above size pos))
  )

  (:action go_down
    :effect (:set pos (below size pos))
  )

  (:action go_right
    :effect (:set pos (right size pos))
  )

  (:action go_left
    :effect (:set pos (left size pos))
  )

  (:action go_in
    :effect (:set pos (in size pos))
  )

  (:action go_out
    :effect (:set pos (out size pos))
  )
)

(define (problem p2)
  (:domain cube)
  (:init (:set size 2)
         (:set pos :in :integer[1,8]))
  (:goal (= pos 8)))

(define (problem p3)
  (:domain cube)
  (:init (:set size 3)
         (:set pos :in :integer[1,27]))
  (:goal (= pos 27)))

(define (problem p4)
  (:domain cube)
  (:init (:set size 4)
         (:set pos :in :integer[1,64]))
  (:goal (= pos 64)))

(define (problem p5)
  (:domain cube)
  (:init (:set size 5)
         (:set pos :in :integer[1,125]))
  (:goal (= pos 125)))

(define (problem p6)
  (:domain cube)
  (:init (:set size 6)
         (:set pos :in :integer[1,216]))
  (:goal (= pos 216)))

(define (problem p7)
  (:domain cube)
  (:init (:set size 7)
         (:set pos :in :integer[1,343]))
  (:goal (= pos 343)))

(define (problem p8)
  (:domain cube)
  (:init (:set size 8)
         (:set pos :in :integer[1,512]))
  (:goal (= pos 512)))

(define (problem p9)
  (:domain cube)
  (:init (:set size 9)
         (:set pos :in :integer[1,729]))
  (:goal (= pos 729)))

(define (problem p10)
  (:domain cube)
  (:init (:set size 10)
         (:set pos :in :integer[1,1000]))
  (:goal (= pos 1000)))

(define (problem p15)
  (:domain cube)
  (:init (:set size 15)
         (:set pos :in :integer[1,3375]))
  (:goal (= pos 3375)))
