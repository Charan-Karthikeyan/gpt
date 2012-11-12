(define (domain sort)
  (:model (:dynamics :non-deterministic) (:feedback :partial))
  (:objects array - :array[5] :integer[1,5])

  (:ramification ram1
    :formula (:and (:not (= array[0] array[1]))
                   (:not (= array[0] array[2]))
                   (:not (= array[0] array[3]))
                   (:not (= array[0] array[4]))
                   (:not (= array[1] array[2]))
                   (:not (= array[1] array[3]))
                   (:not (= array[1] array[4]))
                   (:not (= array[2] array[3]))
                   (:not (= array[2] array[4]))
                   (:not (= array[3] array[4]))))

  (:action cmp
    :parameters ?i ?j - :integer[0,4]
    :precondition (< ?i ?j)
    :observation (< array[?i] array[?j]))

  (:action swap
    :parameters ?i ?j - :integer[0,4]
    :precondition (< ?i ?j)
    :effect (:set array[?i] array[?j])
            (:set array[?j] array[?i])))

;; sort 5 elements
(define (problem p5)
  (:domain sort)
  (:init (:set array[0] :in { 1 2 3 4 5 })
         (:set array[1] :in { 1 2 3 4 5 })
         (:set array[2] :in { 1 2 3 4 5 })
         (:set array[3] :in { 1 2 3 4 5 })
         (:set array[4] :in { 1 2 3 4 5 }))
  (:goal (:and (< array[0] array[1])
               (< array[1] array[2])
               (< array[2] array[3])
               (< array[3] array[4]))))

