package com.example.smartplantwateringsystem;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;

public class fragment2 extends Fragment {

    View inflatedview = null;
    private DatabaseReference databaseReference;

    private Button selectButton;
    private TextView percentage_level;
    private SeekBar moisture_bar;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        // Inflate the layout for this fragment
        inflatedview = inflater.inflate(R.layout.fragment_fragment2, container, false);

        databaseReference = FirebaseDatabase.getInstance().getReference();

        selectButton  = (Button)inflatedview.findViewById(R.id.select_button);
        moisture_bar = (SeekBar) inflatedview.findViewById(R.id.seekBar5);
        percentage_level = (TextView) inflatedview.findViewById(R.id.textView3);


        databaseReference.child("ideal_moisture").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                percentage_level.setText(String.valueOf(snapshot.getValue())+" %");
                moisture_bar.setProgress(Integer.parseInt(String.valueOf(snapshot.getValue())));
            }
            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });


        selectButton.setOnClickListener( new View.OnClickListener() {

            @Override
            public void onClick(View v) {

                databaseReference.child("pump").setValue(true);
            }
        });


        moisture_bar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                percentage_level.setText(String.valueOf(moisture_bar.getProgress())+" %");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

                databaseReference.child("ideal_moisture").setValue(moisture_bar.getProgress());
            }
        });

        databaseReference.child("pump").addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                if(snapshot.exists()) {
                    Boolean status = (Boolean) snapshot.getValue();

                    if (!status) {
                        selectButton.setText("Pump Water");

                    } else if (status) {
                        selectButton.setText("Pump scheduled");
                    }
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
            }
        });

        return inflatedview;
    }
}