/*
 * Copyright (c) 2020. Laurent Réveillère
 */

package fr.ubx.poo.ubomb.engine;


public class Timer {
    //Classe permettant de créer et gérer un timer
    private final long duration;
    private long startTime;
    private boolean running = false;
    private boolean requested = false;
    private long remaining;

    // Set a timer for a duration in ms
    public Timer(long duration) {
        this.duration = duration;
        remaining = duration;
    }

    public void update(long now) {
        // time is in ns
        if (running) {
            remaining = duration - (now - startTime) / 1000000;
            if (remaining < 0) {
                running = false;
            }
        } else if (requested) {
            running = true;
            requested = false;
            startTime = now;
            remaining = duration;
        }
    }

    public long remaining() {
        return remaining;
    }

    public void start() {
        if (!running)
            requested = true;
    }

    public void end(){
        remaining = 0;
        running = false;
        requested = false;
    }

    public boolean notIsRunning() {
        return !running && !requested;
    }

    @Override
    public String toString() {
        return String.valueOf(remaining);
    }
}
