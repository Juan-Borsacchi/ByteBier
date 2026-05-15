-- Database: ByteBier

DROP DATABASE IF EXISTS "ByteBier";

CREATE DATABASE "ByteBier"
    WITH
    OWNER = postgres
    ENCODING = 'UTF8'
    LC_COLLATE = 'Portuguese_Brazil.1252'
    LC_CTYPE = 'Portuguese_Brazil.1252'
    LOCALE_PROVIDER = 'libc'
    TABLESPACE = pg_default
    CONNECTION LIMIT = -1
    IS_TEMPLATE = False;

ALTER DATABASE "ByteBier"
    SET "TimeZone" TO 'America/Sao_Paulo';

-- SEQUENCE: public.sensor_readings_id_seq
CREATE SEQUENCE IF NOT EXISTS public.sensor_readings_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE public.sensor_readings_id_seq
    OWNER TO postgres;

-- Table: public.sensor_readings
DROP TABLE IF EXISTS public.sensor_readings;

CREATE TABLE IF NOT EXISTS public.sensor_readings
(
    id integer NOT NULL DEFAULT nextval('sensor_readings_id_seq'::regclass),
    grupo_id integer NOT NULL,
    "timestamp" timestamp with time zone DEFAULT CURRENT_TIMESTAMP,
    temp_amb double precision,
    temp_liq double precision,
    umidade double precision,
    etapa character varying (30) COLLATE pg_catalog."default",
    device_id character varying (50) COLLATE pg_catalog."default" DEFAULT 'ESP32_ByteBier'::character varying,
    CONSTRAINT sensor_readings_pkey PRIMARY KEY (id)
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.sensor_readings
    OWNER to postgres;

ALTER SEQUENCE public.sensor_readings_id_seq
    OWNED BY public.sensor_readings.id;

-- SEQUENCE: public.alerts_id_seq
DROP SEQUENCE IF EXISTS public.alerts_id_seq;

CREATE SEQUENCE IF NOT EXISTS public.alerts_id_seq
    INCREMENT 1
    START 1
    MINVALUE 1
    MAXVALUE 2147483647
    CACHE 1;

ALTER SEQUENCE public.alerts_id_seq
    OWNER TO postgres;

-- Table: public.alerts
DROP TABLE IF EXISTS public.alerts;

CREATE TABLE IF NOT EXISTS public.alerts
(
    id integer NOT NULL DEFAULT nextval('alerts_id_seq'::regclass),
    reading_id integer,
    grupo_id integer NOT NULL,
    tipo character varying (20) COLLATE pg_catalog."default",
    severidade character varying (10) COLLATE pg_catalog."default",
    mensagem text COLLATE pg_catalog."default",
    valor double precision,
    threshold text COLLATE pg_catalog."default",
    created_at timestamp with time zone DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT alerts_pkey PRIMARY KEY (id),
    CONSTRAINT alerts_reading_id_fkey FOREIGN KEY (reading_id)
        REFERENCES public.sensor_readings (id) MATCH SIMPLE
        ON UPDATE NO ACTION
        ON DELETE CASCADE
)
TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.alerts
    OWNER to postgres;

ALTER SEQUENCE public.alerts_id_seq
    OWNED BY public.alerts.id;
