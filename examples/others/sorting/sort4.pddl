(define (domain sort)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:objects array - :array[4] :integer[1,4])

  (:action cmp
    :parameters ?i ?j - :integer[0,3]
    :precondition (< ?i ?j)
    :observation (< array[?i] array[?j]))

  (:action swap
    :parameters ?i ?j - :integer[0,3]
    :precondition (< ?i ?j)
    :effect (:set array[?i] array[?j])
            (:set array[?j] array[?i])))

;; sort 4 elements
(define (problem p4)
  (:domain sort)
  (:init (:set array[0] :in { 1 2 3 4 })
         (:set array[1] :in { 1 2 3 4 })
         (:not (= array[1] array[0]))
         (:set array[2] :in { 1 2 3 4 })
         (:and (:not (= array[2] array[0]))
               (:not (= array[2] array[1])))
         (:set array[3] :in { 1 2 3 4 })
         (:and (:not (= array[3] array[0]))
               (:not (= array[3] array[1]))
               (:not (= array[3] array[2])))
  )
  (:goal (:and (< array[0] array[1])
               (< array[1] array[2])
               (< array[2] array[3]))))
