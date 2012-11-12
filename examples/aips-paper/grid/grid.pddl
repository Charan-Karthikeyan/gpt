(define (domain grid1)
  (:model (:dynamics :probabilistic) (:feedback :partial))
  (:external (:function above :integer[0,10] :integer[0,10])
             (:function below :integer[0,10] :integer[0,10])
             (:function right :integer[0,10] :integer[0,10])
             (:function left  :integer[0,10] :integer[0,10])
  )
  (:objects pos map goal penalty - :integer[0,10])

  (:action go_up
    :precondition (:not (= (above pos) 10))
    :effect (:set pos (above pos))
    :observation
      (:probabilistic
         (1.00 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.00 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (above pos) penalty) 50 1)
  )

  (:action go_down
    :precondition (:not (= (below pos) 10))
    :effect (:set pos (below pos))
    :observation
      (:probabilistic
         (1.00 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.00 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (below pos) penalty) 50 1)
  )

  (:action go_right
    :precondition (:not (= (right pos) 10))
    :effect (:set pos (right pos))
    :observation
      (:probabilistic
         (1.00 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.00 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (right pos) penalty) 50 1)
  )

  (:action go_left
    :precondition (:not (= (left pos) 10))
    :effect (:set pos (left pos))
    :observation
      (:probabilistic
         (1.00 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.00 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (left pos) penalty) 50 1)
  )
)

(define (problem p1)
  (:domain grid1)
  (:init (:set pos 6)
         (:set map 9)
         (:set goal :in { 0 4 })
         (:set penalty :in { 0 4 })
         (:not (= goal penalty)))
  (:goal (= pos goal)))



(define (domain grid2)
  (:model (:dynamics :probabilistic) (:feedback :partial))
  (:external (:function above :integer[0,10] :integer[0,10])
             (:function below :integer[0,10] :integer[0,10])
             (:function right :integer[0,10] :integer[0,10])
             (:function left  :integer[0,10] :integer[0,10])
  )
  (:objects pos map goal penalty - :integer[0,10])

  (:action go_up
    :precondition (:not (= (above pos) 10))
    :effect (:set pos (above pos))
    :observation
      (:probabilistic
         (0.75 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.25 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (above pos) penalty) 50 1)
  )

  (:action go_down
    :precondition (:not (= (below pos) 10))
    :effect (:set pos (below pos))
    :observation
      (:probabilistic
         (0.75 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.25 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (below pos) penalty) 50 1)
  )

  (:action go_right
    :precondition (:not (= (right pos) 10))
    :effect (:set pos (right pos))
    :observation
      (:probabilistic
         (0.75 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.25 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (right pos) penalty) 50 1)
  )

  (:action go_left
    :precondition (:not (= (left pos) 10))
    :effect (:set pos (left pos))
    :observation
      (:probabilistic
         (0.75 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.25 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (left pos) penalty) 50 1)
  )
)

(define (problem p2)
  (:domain grid2)
  (:init (:set pos 6)
         (:set map 9)
         (:set goal :in { 0 4 })
         (:set penalty :in { 0 4 })
         (:not (= goal penalty)))
  (:goal (= pos goal)))



(define (domain grid3)
  (:model (:dynamics :probabilistic) (:feedback :partial))
  (:external (:function above :integer[0,10] :integer[0,10])
             (:function below :integer[0,10] :integer[0,10])
             (:function right :integer[0,10] :integer[0,10])
             (:function left  :integer[0,10] :integer[0,10])
  )
  (:objects pos map goal penalty - :integer[0,10])

  (:action go_up
    :precondition (:not (= (above pos) 10))
    :effect (:set pos (above pos))
    :observation
      (:probabilistic
         (0.50 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.50 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (above pos) penalty) 50 1)
  )

  (:action go_down
    :precondition (:not (= (below pos) 10))
    :effect (:set pos (below pos))
    :observation
      (:probabilistic
         (0.50 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.50 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (below pos) penalty) 50 1)
  )

  (:action go_right
    :precondition (:not (= (right pos) 10))
    :effect (:set pos (right pos))
    :observation
      (:probabilistic
         (0.50 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.50 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (right pos) penalty) 50 1)
  )

  (:action go_left
    :precondition (:not (= (left pos) 10))
    :effect (:set pos (left pos))
    :observation
      (:probabilistic
         (0.50 (:when (= pos map) goal)
               (:when (= pos goal) goal))
         (0.50 (:when (= pos map) penalty)
               (:when (= pos goal) goal)))
    :cost (:if (= (left pos) penalty) 50 1)
  )
)

(define (problem p3)
  (:domain grid3)
  (:init (:set pos 6)
         (:set map 9)
         (:set goal :in { 0 4 })
         (:set penalty :in { 0 4 })
         (:not (= goal penalty)))
  (:goal (= pos goal)))

