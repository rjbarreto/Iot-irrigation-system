package com.example.smartplantwateringsystem;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;


public class fragment1 extends Fragment {

    private DatabaseReference databaseReference;
    View inflatedview = null;
    private TextView tempvaltxt;
    private TextView lumvaltxt;
    private TextView humvaltxt;
    private TextView moistvaltxt;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        // Inflate the layout for this fragment
        inflatedview = inflater.inflate(R.layout.fragment_fragment1, container, false);

        tempvaltxt  = (TextView)inflatedview.findViewById(R.id.tempval);
        moistvaltxt = (TextView)inflatedview.findViewById(R.id.moistval);
        humvaltxt = (TextView)inflatedview.findViewById(R.id.humval);
        lumvaltxt = (TextView)inflatedview.findViewById(R.id.lumval);

        databaseReference = FirebaseDatabase.getInstance().getReference();


        databaseReference.child("temperature").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                tempvaltxt.setText(String.valueOf(snapshot.getValue())+" ºC");
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                tempvaltxt.setText("Error");
            }
        });


        databaseReference.child("moisture").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                float current_level= Float.parseFloat(String.valueOf(snapshot.getValue()));
                moistvaltxt.setText(String.valueOf(snapshot.getValue())+" %");
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                moistvaltxt.setText("Error");
            }
        });


        databaseReference.child("humidity").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                humvaltxt.setText(String.valueOf(snapshot.getValue())+" g/kg");
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                humvaltxt.setText("Error");
            }
        });


        databaseReference.child("luminosity").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                lumvaltxt.setText(String.valueOf(snapshot.getValue())+" lux");
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                lumvaltxt.setText("Error");
            }
        });


        return inflatedview;
    }
}
