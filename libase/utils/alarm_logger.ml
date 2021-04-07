module type S = sig
  val fatal: ?e:exn -> ('a, Format.formatter, unit, 'b) format4 -> 'a
  val error: ('a, Format.formatter, unit) format -> 'a
  val result: ('a, Format.formatter, unit) format -> 'a
  val warning: ?level:int -> ('a, Format.formatter, unit) format -> 'a

  val info: ?level:int -> ('a, Format.formatter,unit) format -> 'a

  val debug: ?level:int -> ('a, Format.formatter, unit) format -> 'a
  val fdebug: ?level:int -> (unit -> (unit, Format.formatter, unit) format) -> unit

  val check : string -> unit
    (** Emit a check notification, i.e. inform the log reader that an alarm may
       follow. *)

  val alarm : string -> unit
    (** Emit an alarm, i.e. a notification that the code could not be proved
       conform to the specification. *)
end
