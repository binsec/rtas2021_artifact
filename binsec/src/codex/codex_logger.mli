val current_instr_addr : Virtual_address.t ref

module Alarm_record : sig
  type t
  val empty : t
  val check : string -> t -> t
  val alarm : string -> t -> t
  val pretty : unique:bool -> Format.formatter -> t -> unit
  val new_phase : string -> t -> t
  val total_alarms : ignore:(string list) -> ?ignore_phase:(string list) ->
    unique:bool -> t -> int
end

module type Binsec_logger_S = sig
  include Logger.S
  val check : string -> unit
    (** Emit a check notification, i.e. inform the log reader that an alarm may
       follow. *)

  val alarm : string -> unit
    (** Emit an alarm, i.e. a notification that the code could not be proved
       conform to the specification. *)

  val alarm_record : unit -> Alarm_record.t
  val switch_to_phase : string -> unit
end

include Binsec_logger_S

(** Logger to be used specifically by the libase library. *)
module Codex_logger : Codex.Codex_log.S
