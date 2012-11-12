(define (domain square)
  (:model (:dynamics :non-deterministic) (:feedback :null))
  (:external (:function above (:integer[1,100] :integer[1,10000]) :integer[1,10000])
             (:function below (:integer[1,100] :integer[1,10000]) :integer[1,10000])
             (:function left  (:integer[1,100] :integer[1,10000]) :integer[1,10000])
             (:function right (:integer[1,100] :integer[1,10000]) :integer[1,10000])
  )
  (:objects size - :integer[1,100]
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
)


(define (problem p2)
  (:domain square)
  (:init (:set size 2)
         (:set pos :in :integer[1,4]))
  (:goal (= pos 4)))

(define (problem p4)
  (:domain square)
  (:init (:set size 4)
         (:set pos :in :integer[1,16]))
  (:goal (= pos 16)))

(define (problem p6)
  (:domain square)
  (:init (:set size 6)
         (:set pos :in :integer[1,36]))
  (:goal (= pos 36)))

(define (problem p8)
  (:domain square)
  (:init (:set size 8)
         (:set pos :in :integer[1,64]))
  (:goal (= pos 64)))

(define (problem p10)
  (:domain square)
  (:init (:set size 10)
         (:set pos :in :integer[1,100]))
  (:goal (= pos 100)))

(define (problem p12)
  (:domain square)
  (:init (:set size 12)
         (:set pos :in :integer[1,144]))
  (:goal (= pos 144)))

(define (problem p14)
  (:domain square)
  (:init (:set size 14)
         (:set pos :in :integer[1,196]))
  (:goal (= pos 196)))

(define (problem p16)
  (:domain square)
  (:init (:set size 16)
         (:set pos :in :integer[1,256]))
  (:goal (= pos 256)))

(define (problem p18)
  (:domain square)
  (:init (:set size 18)
         (:set pos :in :integer[1,324]))
  (:goal (= pos 324)))

(define (problem p20)
  (:domain square)
  (:init (:set size 20)
         (:set pos :in :integer[1,400]))
  (:goal (= pos 400)))


;; limiting


(define (problem p21)
  (:domain square)
  (:init (:set size 21)
         (:set pos :in :integer[1,441]))
  (:goal (= pos 441)))

(define (problem p22)
  (:domain square)
  (:init (:set size 22)
         (:set pos :in :integer[1,484]))
  (:goal (= pos 484)))

(define (problem p40)
  (:domain square)
  (:init (:set size 40)
         (:set pos :in :integer[1,1600]))
  (:goal (= pos 1600)))
