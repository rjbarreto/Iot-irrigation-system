package com.example.smartplantwateringsystem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.viewpager.widget.ViewPager;

import android.os.Bundle;
import android.widget.Toast;

import com.google.android.material.tabs.TabLayout;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    private TabLayout tablayout;
    private ViewPager viewPager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tablayout=findViewById(R.id.tablayout);
        viewPager=findViewById(R.id.viewpager);

        tablayout.setupWithViewPager(viewPager);

        VPAdapter vpAdapter=new VPAdapter(getSupportFragmentManager(), FragmentPagerAdapter.BEHAVIOR_RESUME_ONLY_CURRENT_FRAGMENT);

        vpAdapter.addFragment(new fragment1(), "Sensors");
        vpAdapter.addFragment(new fragment2(), "Water pump");

        viewPager.setAdapter(vpAdapter);
    }
}